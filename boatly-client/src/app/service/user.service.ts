import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { Boat } from '../model/boat';
import { User } from '../model/user';

@Injectable({
  providedIn: 'root'
})
export class UserService {

  constructor(private http: HttpClient) { }

  public getUsers(): Observable<User[]> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', "Bearer "+ token);
    
    return this.http.get<User[]>("http://localhost:8080/users", {headers});
  }
}
