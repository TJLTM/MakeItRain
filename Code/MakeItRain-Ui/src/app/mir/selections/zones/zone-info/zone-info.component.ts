import { Component, OnInit, Input, ViewChild } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';

export interface ScheduleElement {
  enable: boolean;
  days: Array<string>;
  startTime: string;
  duration: number;
}

@Component({
  selector: 'app-zone-info',
  templateUrl: './zone-info.component.html',
  styleUrls: ['./zone-info.component.scss']
})
export class ZoneInfoComponent implements OnInit {
  @Input() activeZone!: number | string;
  dataSource: ScheduleElement[] = [
    {enable: true, days: ['Mon', 'Tues'], startTime:'6:00PM', duration: 6},
    {enable: true, days: ['Tues', 'Thurs'], startTime:'5:00AM', duration: 3}
  ];

  constructor(public dialog: MatDialog) { }

  ngOnInit(): void {
  }

  displayedColumns: string[] = ['Enable', 'Days', 'Start Time', 'Duration', 'Options'];

  runManualDialog(){
    
  }

}

