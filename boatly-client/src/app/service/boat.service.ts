import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Boat } from '../model/boat';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { DetectedStolen } from '../model/detected-stolen';
import { Trip } from '../model/trip';

@Injectable({
  providedIn: 'root'
})
export class BoatService {

  constructor(private http: HttpClient) { }

  public getBoats(): Observable<Boat[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);

    return this.http.get<Boat[]>("http://localhost:8080/boats", {headers});
  }

  public getBoatsById(id: string): Observable<Boat[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);

    return this.http.get<Boat[]>("http://localhost:8080/boats", {headers});
  }

  public getMyBoats(): Observable<Boat[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);

    return this.http.get<Boat[]>("http://localhost:8080/boats/info", {headers});
  }

  public startTrip(boatId: string): Observable<Boat> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);
    const params = new HttpParams().set("boatId", boatId)


    return this.http.post<Boat>("http://localhost:8080/trips", {},  {headers: headers, params: params});
  }

  public insertBoat(boatId: string, boatName: string): Observable<Boat> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);
    //const params = new HttpParams().set("boatId", boatId).set("boatName", boatName);

    const body = { id: boatId, name: boatName };

    return this.http.post<Boat>("http://localhost:8080/boats", body,  {headers: headers});
  }

  public updateBoat(boat: Boat): Observable<Boat> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);
    //const params = new HttpParams().set("boatId", boatId).set("boatName", boatName);

    const body = boat;

    return this.http.patch<Boat>("http://localhost:8080/boats", body,  {headers: headers});
  }

  public getDetectedStolens(): Observable<DetectedStolen[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);

    return this.http.get<DetectedStolen[]>("http://localhost:8080/detected-stolens", {headers});
  }

  public getTrips(): Observable<Trip[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);

    return this.http.get<Trip[]>("http://localhost:8080/trips", {headers});
  }
}
