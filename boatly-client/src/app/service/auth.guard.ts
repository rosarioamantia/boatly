import { Injectable } from '@angular/core';
import { ActivatedRouteSnapshot, CanActivate, Router, RouterStateSnapshot, UrlTree } from '@angular/router';
import { Observable } from 'rxjs';
import { AuthService } from './auth.service';

@Injectable({
  providedIn: 'root'
})
export class AuthGuard implements CanActivate {
  constructor(private authService: AuthService, private router: Router) {}

  canActivate(route: ActivatedRouteSnapshot): boolean {
    if (this.authService.getToken()) {
      const expectedRole = route.data['role']; // Ruolo richiesto dalla rotta
      const userRole = this.authService.getRole();

      if(expectedRole == userRole){
        return true;
      }else{
        return false;
      }
    } else {
      this.router.navigate(['/login']);
      return false;
    }
  }
}
