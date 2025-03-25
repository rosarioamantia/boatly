import { Component, OnDestroy, OnInit } from '@angular/core';
import { MatFormFieldModule } from '@angular/material/form-field';
import { AuthService } from '../service/auth.service';
import { Boat } from '../model/boat';
import { BoatService } from '../service/boat.service';
import { UserService } from '../service/user.service';
import { User } from '../model/user';
import { WebSocketService } from '../service/web-socket.service';
import { DetectedStolen } from '../model/detected-stolen';
import { Trip } from '../model/trip';

@Component({
  selector: 'app-admin',
  templateUrl: './admin.component.html',
  styleUrls: ['./admin.component.scss'],
})
export class AdminComponent implements OnInit, OnDestroy {
  boats: Boat[] = [];
  users: User[] = [];
  detectedStolens: DetectedStolen[] = [];
  trips: Trip[] = [];


  constructor(
    private authService: AuthService,
    private boatService: BoatService,
    private userService: UserService,
    private webSocketService: WebSocketService) {
      this.webSocketService.connect();
    }

  ngOnInit(): void {
    this.getInitialData();

    this.webSocketService.getBoatUpdates().subscribe((updatedBoat) => {
      if (updatedBoat) {
        const index = this.boats.findIndex(p => p.id === updatedBoat.id);
        if (index !== -1) {
          this.boats[index] = updatedBoat;
        }
      }
    });

    this.webSocketService.getDetectedStolenUpdates().subscribe((detectedStolen) => {
      if (detectedStolen) {
        this.detectedStolens.unshift(detectedStolen);
      }
    });

    this.webSocketService.getTripUpdates().subscribe((trip) => {
      if (trip) {
        this.trips.unshift(trip);
      }
    });
  }

  public getInitialData() {
    this.getBoats();
    this.getUsers();
    this.getDetectedStolens();
    this.getTrips();
  }

  public getBoats(): void {
    this.boatService.getBoats().subscribe(
      (response: Boat[]) => {
        this.boats = response;
      }
    )
  }

  public getUsers(): void {
    this.userService.getUsers().subscribe(
      (response: User[]) => {
        this.users = response;
      }
    )
  }

  public getDetectedStolens(): void {
    this.boatService.getDetectedStolens().subscribe(
      (response: DetectedStolen[]) => {
        this.detectedStolens = response.reverse();
      }
    )
  }

  public getTrips(): void {
    this.boatService.getTrips().subscribe(
      (response: Trip[]) => {
        this.trips = response.reverse();
      }
    )
  }

  public onResolveBoat(boat: Boat) {
    boat.inHarbor = true;
    boat.stolen = false;
    this.boatService.updateBoat(boat).subscribe(
      (response: Boat) => {
        alert("Boat " + boat.name + " set as resolved");
      }
    )
  }

  ngOnDestroy(): void {
    this.webSocketService.disconnect();
  }

}
