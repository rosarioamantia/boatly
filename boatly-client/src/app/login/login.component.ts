import { Component } from '@angular/core';
import { NgIf } from '@angular/common';
import { FormControl, FormGroup, FormsModule, Validators } from '@angular/forms';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { AuthService } from '../service/auth.service';
import { Router } from '@angular/router';
import { MatDividerModule } from '@angular/material/divider';


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

  title = 'angular-login-example';

  constructor(private authService: AuthService, private router: Router) {
    this.form = new FormGroup({
      usernameControl: new FormControl('', [Validators.required]),
      passwordControl: new FormControl('', [Validators.required])
    });
  }

  public onSubmit() {
    if (this.form.valid) {
      alert("entrato");
      const formData = this.form.value;

      this.username = formData.usernameControl;
      this.password = formData.passwordControl;

      this.authService.login(this.username!, this.password!).subscribe({
        next: (response) => {
          this.authService.saveToken(response.token);
          this.authService.saveRole(response.role);

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

  ngOnInit(): void { }
}
