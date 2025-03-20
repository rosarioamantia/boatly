import { Component, OnInit } from '@angular/core';
import { Boat } from '../model/boat';
import { BoatService } from '../service/boat.service';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { MatSlideToggleChange } from '@angular/material/slide-toggle';

@Component({
  selector: 'app-user',
  templateUrl: './user.component.html',
  styleUrls: ['./user.component.scss']
})
export class UserComponent implements OnInit {
  boats: Boat[] = [];
  registerBoatForm: FormGroup;

  constructor(private boatService: BoatService) {
    this.registerBoatForm = new FormGroup({
      idControl: new FormControl('', [Validators.required, Validators.minLength(6), Validators.maxLength(6)]),
      nameControl: new FormControl('', [Validators.required])
    })
  }

  ngOnInit(): void {
    this.getInitialData();
  }

  public onToggleChange(value: MatSlideToggleChange, boatId: string): void {
    this.startTrip(boatId);
  }

  public getInitialData() {
    this.getMyBoats();
  }

  public getMyBoats(): void {
    this.boatService.getMyBoats().subscribe(
      (response: Boat[]) => {
        this.boats = response;
      }
    )
  }

  public startTrip(boatId: string): void {
    this.boatService.startTrip(boatId).subscribe(
      (response: Boat) => {
        this.boats.forEach(boat => {
          if (boat.id == boatId) {
            const indexElementToReplace = this.boats.indexOf(boat);
            this.boats[indexElementToReplace] = response;
          }
        });
      }
    )
  }

  public onInsertBoatSubmit(): void {
    if (this.registerBoatForm.valid) {
      const formData = this.registerBoatForm.value;

      const boatId = formData.idControl;
      const boatName = formData.nameControl;
      
      this.boatService.insertBoat(boatId, boatName).subscribe(
        (response: Boat) => {
          this.boats.push(response);
        }
      )


    }
  }

}
