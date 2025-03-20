import { Component, OnInit } from '@angular/core';
import { MatFormFieldModule } from '@angular/material/form-field';
import { AuthService } from '../service/auth.service';
import { Boat } from '../model/boat';
import { BoatService } from '../service/boat.service';
import { UserService } from '../service/user.service';
import { User } from '../model/user';

@Component({
  selector: 'app-admin',
  templateUrl: './admin.component.html',
  styleUrls: ['./admin.component.scss'],
})
export class AdminComponent implements OnInit {
  boats: Boat[] = [];
  users: User[] = [];

  constructor(
    private authService: AuthService,
    private boatService: BoatService, 
    private userService: UserService) { }

  ngOnInit(): void {
    this.getInitialData();
  }

  public getInitialData(){
    this.getBoats();
    this.getUsers();
  }

  public getBoats(): void{
    this.boatService.getBoats().subscribe(
      (response: Boat[]) => {
        this.boats = response;
      }
    )
  }

  public getUsers(): void{
    this.userService.getUsers().subscribe(
      (response: User[]) => {
        this.users = response;
      }
    )
  }

}
