const libraryWEBRTC = {
  $WEBRTC: {
    reset: function (mode) {
      WEBRTC.mode = mode || null;
      WEBRTC.messageHandlers = [];
      if (WEBRTC.peers) {
        WEBRTC.peers.forEach(peer => peer && peer.pc.close());
      }
      WEBRTC.peers = [];
      for (let i = 0; i < WEBRTC.maxPeers; i++) {
        WEBRTC.peers.push(undefined);
      }
      if (WEBRTC.ws) WEBRTC.ws.close();
      WEBRTC.ws = null;
    },
    addPeer: function (peerId, pc) {
      const index = WEBRTC.peers.findIndex(peer => peer === undefined);
      if (index === -1) return undefined;

      const peer = {
        peerId,
        pc,
        makingOffer: false,
        ignoreOffer: false,
        dataChannel: null,
        messages: [],
      }
      WEBRTC.peers[index] = peer;
      return peer;
    },
    peerSlotsAvailable: function () {
      return WEBRTC.peers.some(peer => peer === undefined);
    },
    findPeerById: function (peerId) {
      if (WEBRTC.mode === 'client') return WEBRTC.peers[0];
      return WEBRTC.peers.find(peer => peer && peer.peerId === peerId);
    },
    closeAndRemovePeer: function (peer) {
      peer.pc.close();
      const index = WEBRTC.peers.findIndex(other => other && other.peerId === peer.peerId);
      if (index !== -1) {
        WEBRTC.peers[index] = undefined;
      }
    },
    send: function (message) {
      WEBRTC.ws.send(JSON.stringify(message));
    },
    connectToLobby: function () {
      const scheme = window.location.protocol === 'https:' ? 'wss' : 'ws'
      WEBRTC.ws = new WebSocket(scheme + '://' + WEBRTC.lobbyHost);
      WEBRTC.ws.addEventListener('open', () => {
        WEBRTC.dispatchMessage({ type: 'connectedToLobby' });
      });
      WEBRTC.ws.addEventListener('close', () => {
        WEBRTC.dispatchMessage({ type: 'lobbyConnectionLost' });
      });
      WEBRTC.ws.addEventListener('message', event => {
        WEBRTC.dispatchMessage(JSON.parse(event.data));
      });
    },
    dispatchMessage: function (message) {
      WEBRTC.messageHandlers.forEach(messageHandler => messageHandler(message));
    },
    addMessageHandler: function (fn) {
      WEBRTC.messageHandlers.push(fn);
      return () => WEBRTC.removeMessageHandler(fn);
    },
    removeMessageHandler: function (fn) {
      WEBRTC.messageHandlers = WEBRTC.messageHandlers.filter(h => h !== fn);
    },
    waitForMessage: function (messageType, timeoutMs) {
      return new Promise((resolve, reject) => {
        const done = message => {
          unsubscribe();
          clearTimeout(timeoutHandle);
          resolve(message);
        }
        const fail = () => {
          unsubscribe();
          reject();
        }
        const handler = message => {
          if (message.type === messageType) done(message)
        }
        const unsubscribe = WEBRTC.addMessageHandler(handler);
        const timeoutHandle = setTimeout(fail, timeoutMs || 5000);
      })
    },

    // peerId is undefined if mode === 'client'
    startWebrtcNegotiation: function (peerId) {
      // See https://developer.mozilla.org/en-US/docs/Web/API/WebRTC_API/Perfect_negotiation
      console.log('Start WebRTC negotiation', peerId !== undefined ? 'with ' + peerId : '');

      const pc = new RTCPeerConnection({
        iceServers: [{
          urls: 'stun:stun.l.google.com:19302' // Google's public STUN server
        }]
      });

      const peer = WEBRTC.addPeer(peerId, pc);
      if (!peer) {
        // No more connection slots
        pc.close();
        return;
      }

      pc.addEventListener('icecandidate', event => {
        WEBRTC.send({
          type: 'webrtcSignaling',
          clientId: peerId,
          candidate: event.candidate,
        });
      });
      pc.addEventListener('negotiationneeded', () => {
        peer.makingOffer = true;
        pc.setLocalDescription().then(() => {
          WEBRTC.send({
            type: 'webrtcSignaling',
            clientId: peerId,
            description: pc.localDescription,
          });
          peer.makingOffer = false;
        }).catch(() => {
          peer.makingOffer = false;
        })
      });
      if (WEBRTC.mode === 'server') {
        const dataChannel = pc.createDataChannel('data');
        dataChannel.addEventListener('open', () => {
          WEBRTC.setupDataChannel(peer, dataChannel);
        });
      } else {
        pc.addEventListener('datachannel', event => {
          WEBRTC.setupDataChannel(peer, event.channel);
          WEBRTC.dispatchMessage({ type: 'connected' })
        });
      }
    },
    handleWebrtcSignaling: function (message) {
      // See https://developer.mozilla.org/en-US/docs/Web/API/WebRTC_API/Perfect_negotiation
      const peer = WEBRTC.findPeerById(message.clientId);
      const pc = peer.pc;

      if (message.description) {
        const description = message.description;

        const offerCollision =
          description.type === 'offer' &&
          (peer.makingOffer || pc.signalingState !== 'stable');

        peer.ignoreOffer = WEBRTC.mode === 'server' && offerCollision;
        if (peer.ignoreOffer) {
          return;
        }

        pc.setRemoteDescription(description).then(() => {
          if (description.type === 'offer') {
            pc.setLocalDescription().then(() => {
              WEBRTC.send({
                type: 'webrtcSignaling',
                clientId: peer.peerId, // undefined if mode === 'client'
                description: pc.localDescription,
              })
            });
          }
        });
      } else if (message.candidate) {
        pc.addIceCandidate(message.candidate).catch(err => {
          if (!peer.ignoreOffer) {
            throw err;
          }
        })
      }
    },
    setupDataChannel: function (peer, dataChannel) {
      console.log('Data channel ready')
      peer.dataChannel = dataChannel;
      dataChannel.addEventListener('message', event => {
        peer.messages.push(event.data);
      });
      dataChannel.addEventListener('close', () => {
        // null message => peer has disconnected
        peer.messages.push(null);
      })
    }
  },

  // Return true if all required APIs are supported by the browser
  webrtc_init: function (lobbyHost, maxPeers) {
    WEBRTC.lobbyHost = UTF8ToString(lobbyHost);
    WEBRTC.maxPeers = maxPeers;
    fetch('//' + WEBRTC.lobbyHost);
    return !!window.WebSocket &&
      !!window.RTCPeerConnection &&
      !!window.RTCSessionDescription;
  },

  webrtc_deinit: function () {
  },

  webrtc_create_server__deps: ['$Asyncify'],
  webrtc_create_server: function () {
    return Asyncify.handleSleep(wakeup => {
      WEBRTC.reset('server');

      console.log('Connecting to lobby...');
      WEBRTC.connectToLobby();

      WEBRTC.waitForMessage('connectedToLobby').then(() => {
        console.log('Creating game...');
        WEBRTC.send({ type: 'createGame', gameId: 'xyzzy' });
        return WEBRTC.waitForMessage('gameCreated');
      }).then(message => {
        WEBRTC.gameId = message.gameId;
        console.log('Game created with id', message.gameId);
        wakeup(0);
      }).catch(() => {
        if (WEBRTC.ws) {
          WEBRTC.ws.close();
          WEBRTC.ws = null;
        }
        wakeup(1);
      });

      WEBRTC.addMessageHandler(message => {
        if (message.type === 'newClient') {
          if (WEBRTC.peerSlotsAvailable()) {
            WEBRTC.send({
              type: 'acceptJoin',
              gameId: WEBRTC.gameId,
              clientId: message.clientId
            });
            WEBRTC.startWebrtcNegotiation(message.clientId);
          } else {
            WEBRTC.send({
              type: 'rejectJoin',
              gameId: WEBRTC.gameId,
              clientId: message.clientId,
              reason: 'Game is full',
            });
          }
        } else if (message.type === 'webrtcSignaling') {
          WEBRTC.handleWebrtcSignaling(message);
        } else if (message.type === 'clientVanished') {
          const peer = WEBRTC.findPeerById(message.clientId);
          if (peer && !peer.dataChannel) {
            // Signaling not finished yet => remove
            WEBRTC.closeAndRemovePeer(peer);
          }
        }
      });
    });
  },

  webrtc_is_server: function () {
    return WEBRTC.mode === 'server';
  },

  webrtc_connect_to_server__deps: ['$Asyncify'],
  webrtc_connect_to_server: function (gameIdPtr) {
    const gameId = UTF8ToString(gameIdPtr);
    return Asyncify.handleSleep(wakeup => {
      WEBRTC.reset('client');

      console.log('Connecting to lobby...');
      WEBRTC.connectToLobby();

      WEBRTC.waitForMessage('connectedToLobby').then(() => {
        console.log('Joining game', gameId);
        WEBRTC.send({ type: 'joinGame', gameId });
        return WEBRTC.waitForMessage('acceptJoin')
      }).then(() => {
        WEBRTC.startWebrtcNegotiation();
      }).catch(() => {
        if (!WEBRTC.ws) {
          // Already got an error message and closed
          return
        }
        WEBRTC.ws.close();
        WEBRTC.ws = null;
        wakeup(1);
      })

      WEBRTC.addMessageHandler(message => {
        if (message.type === 'webrtcSignaling') {
          WEBRTC.handleWebrtcSignaling(message);
        } else if (message.type === 'connected') {
          WEBRTC.ws.close();  // No need for WebRTC signaling anymore
          WEBRTC.ws = null;
          wakeup(0);
        } else if (message.type === 'error') {
          console.error('Error joining game:', message.reason)
          WEBRTC.ws.close();
          WEBRTC.ws = null;
          wakeup(1);
        }
      });
    })
  },

  webrtc_is_connected: function () {
    const server = WEBRTC.peers[0];
    return server && server.dataChannel && server.dataChannel.readyState === 'open';
  },

  webrtc_has_peer: function (peerIndex) {
    return !!WEBRTC.peers[peerIndex];
  },

  webrtc_reset: function () {
    WEBRTC.reset();
  },

  webrtc_send: function (peerIndex, buffer, bufferSize) {
    const peer = WEBRTC.peers[peerIndex];
    if (!peer || !peer.dataChannel || peer.dataChannel.readyState !== 'open') return -1;

    const message = new ArrayBuffer(bufferSize);
    const view = new Int8Array(message);
    for (let i = 0; i < bufferSize; i++) {
      view[i] = getValue(buffer + i, 'i8');
    }
    peer.dataChannel.send(message);
    return bufferSize;
  },

  webrtc_receive: function (peerIndex, buffer, bufferSize) {
    const peer = WEBRTC.peers[peerIndex];
    if (!peer) return -1;
    if (peer.messages.length === 0) return 0;

    const message = peer.messages.shift();
    if (message === null) {
      // Disconnected
      WEBRTC.closeAndRemovePeer(peer);
      return -1;
    }

    const view = new Int8Array(message);
    if (bufferSize < view.length) {
      throw new Error('buffer too small')
    }
    for (let i = 0; i < view.length; i++) {
      setValue(buffer + i, view[i], 'i8');
    }
    return view.length;
  }
};

autoAddDeps(libraryWEBRTC, '$WEBRTC');
mergeInto(LibraryManager.library, libraryWEBRTC);
