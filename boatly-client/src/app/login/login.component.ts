import { Component } from '@angular/core';
import { NgIf } from '@angular/common';
import { FormControl, FormGroup, FormsModule, Validators } from '@angular/forms';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { AuthService } from '../service/auth.service';
import { Router } from '@angular/router';
import { MatDividerModule } from '@angular/material/divider';
import { WebSocketService } from '../service/web-socket.service';


@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.scss']
})
export class LoginComponent {
  
  username: string | undefined;
  password: string | undefined;
  errorMessage = '';
  form: FormGroup;

  constructor(private authService: AuthService, private router: Router, private webSocketService: WebSocketService) {
    this.form = new FormGroup({
      usernameControl: new FormControl('', [Validators.required]),
      passwordControl: new FormControl('', [Validators.required])
    });
  }

  ngOnInit(): void { }

  public onSubmit() {
    if (this.form.valid) {
      const formData = this.form.value;

      this.username = formData.usernameControl;
      this.password = formData.passwordControl;

      this.authService.login(this.username!, this.password!).subscribe({
        next: (response) => {
          this.authService.saveToken(response.jwtToken);
          this.authService.saveRole(response.role);
          //this.webSocketService.connect();


          if (response.role == "ADMIN") {
            this.router.navigate(['admin/dashboard']);
          } else if (response.role == "USER") {
            this.router.navigate(['user/dashboard']);
          }
        },
        error: () => {
          this.errorMessage = 'Credenziali non valide';
        }
      });
    }
  }

  public onClickSignup(){
    this.router.navigate(['signup']);
  }
}
