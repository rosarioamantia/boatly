import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Router } from '@angular/router';
import { Observable } from 'rxjs';
import { UserDto } from '../model/user-dto';

@Injectable({
  providedIn: 'root'
})
export class AuthService {
  private apiUrl = 'http://localhost:8080/api/auth';

  constructor(private http: HttpClient, private router: Router) {}

  login(username: string, password: string): Observable<any> {
    const token = localStorage.getItem('authToken');
    const headers = new HttpHeaders().set('Authorization', `Bearer ${token}`);
    
    return this.http.post<any>(this.apiUrl + "/login", { username, password });
  }

  registerUser(user: UserDto): Observable<any> {
    return this.http.post<any>(this.apiUrl + "/register",user);
  }

  registerAdmin(username: string, password: string): Observable<any> {
    return this.http.post<any>(this.apiUrl + "/register/admin", { username, password });
  }

  saveToken(token: string): void {
    localStorage.setItem('authToken', token);
  }

  saveRole(role: string): void {
    localStorage.setItem('role', role);
  }

  getToken(): string | null {
    return localStorage.getItem('authToken');
  }

  getRole(): string | null {
    return localStorage.getItem('role');
  }

  logout(): void {
    localStorage.removeItem('authToken');
    localStorage.removeItem('role');
    this.router.navigate(['/login']);
  }
}
