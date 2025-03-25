import { Injectable } from '@angular/core';
import { Client } from '@stomp/stompjs';
import { BehaviorSubject, Observable } from 'rxjs';
import { webSocket, WebSocketSubject } from 'rxjs/webSocket';
import * as SockJS from 'sockjs-client';

@Injectable({
  providedIn: 'root'
})
export class WebSocketService {
  private stompClient!: Client;
  private boatUpdates = new BehaviorSubject<any>(null);
  private tripUpdates = new BehaviorSubject<any>(null);
  private detectedStolenUpdates = new BehaviorSubject<any>(null);

  constructor() {
    //this.connect();
  }

  connect() {
    this.stompClient = new Client({
      webSocketFactory: () => new SockJS('http://localhost:8080/ws'),
      debug: (str) => console.log(str),
      reconnectDelay: 5000,
    });    

    this.stompClient.onConnect = () => {
      console.log('Connected to WebSocket');

      this.stompClient.subscribe('/boatly/boats', (message) => {
        this.boatUpdates.next(JSON.parse(message.body));
      });

      this.stompClient.subscribe('/boatly/trips', (message) => {
        this.tripUpdates.next(JSON.parse(message.body));
      });

      this.stompClient.subscribe('/boatly/detected-stolens', (message) => {
        this.detectedStolenUpdates.next(JSON.parse(message.body));
      });
    };

    this.stompClient.activate();
  }

  getBoatUpdates() {
    return this.boatUpdates.asObservable();
  }

  getTripUpdates() {
    return this.tripUpdates.asObservable();
  }

  getDetectedStolenUpdates() {
    return this.detectedStolenUpdates.asObservable();
  }
  
  disconnect() {
    if(this.stompClient.active){
      this.stompClient.deactivate();
    }
  }
}
