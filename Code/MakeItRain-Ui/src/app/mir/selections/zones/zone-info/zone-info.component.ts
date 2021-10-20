import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-zone-info',
  templateUrl: './zone-info.component.html',
  styleUrls: ['./zone-info.component.scss']
})
export class ZoneInfoComponent implements OnInit {
  @Input() activeZone!: number | string;
  
  constructor() { }

  ngOnInit(): void {
  }

}
