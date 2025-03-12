import { Component } from '@angular/core';
import { AuthService } from './service/auth.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  title = 'Boatly';
  
  constructor(private authService: AuthService, private router: Router) { }

  public logout(){
    this.authService.logout();
  }
}
