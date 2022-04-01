import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss']
})
export class SettingsComponent implements OnInit {
  public editDetails: boolean = false;
  public editNetwork: boolean = false;
  constructor() { }

  public deviceDetails = {
    "DeviceName": "MakeItRain",
    "NodeId": "1"
  }

  ngOnInit(): void {
  }

  editDetailsFunction(event: string){
    this.editDetails = false;
    if(event === 'Save'){

    }else{
      
    }
  }

}
