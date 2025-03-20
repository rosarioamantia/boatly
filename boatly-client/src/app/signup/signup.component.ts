import { Component, OnInit } from '@angular/core';
import { AuthService } from '../service/auth.service';
import { Router } from '@angular/router';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { UserDto } from '../model/user-dto';

@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.scss']
})
export class SignupComponent implements OnInit {
  signupForm: FormGroup;
  username: string | undefined;
  password: string | undefined;
  errorMessage = '';

  constructor(private authService: AuthService, private router: Router) {
    this.signupForm = new FormGroup({
      usernameControl: new FormControl('', [Validators.required]),
      passwordControl: new FormControl('', [Validators.required]),
      emailControl: new FormControl('', [Validators.required]),
      nameControl: new FormControl('', [Validators.required]),
      surnameControl: new FormControl('', [Validators.required]),
      telephoneControl: new FormControl('', [Validators.required])
    });
  }

  ngOnInit(): void {
  }

  public onSubmit() {
    if (this.signupForm.valid) {
      const formData = this.signupForm.value;

      const userDto: UserDto = {
        username: formData.usernameControl,
        password: formData.passwordControl,
        email: formData.emailControl,
        name: formData.nameControl,
        surname: formData.surnameControl,
        phoneNumber: formData.telephoneControl
      }

      this.authService.registerUser(userDto).subscribe({
        complete: () => {
          this.router.navigate(['login']);
          alert("Registration confirmed");       
        },
        error: () => {
          this.errorMessage = 'Credenziali non valide';
        }
      });
    }
  }

  public onClickLogin(){
    this.router.navigate(['login']);
  }

}
