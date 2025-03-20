import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { LoginComponent } from './login/login.component';
import { UserComponent } from './user/user.component';
import { AuthGuard } from './service/auth.guard';
import { AdminComponent } from './admin/admin.component';
import { SignupComponent } from './signup/signup.component';

export const routes: Routes = [
  { path: 'login', component: LoginComponent },
  { path: 'signup', component: SignupComponent},
  { path: 'user/dashboard', component: UserComponent, canActivate: [AuthGuard], data: {role: "USER"} },
  { path: 'admin/dashboard', component: AdminComponent, canActivate: [AuthGuard], data: {role: "ADMIN"} },
  { path: '**', redirectTo: 'login' }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
