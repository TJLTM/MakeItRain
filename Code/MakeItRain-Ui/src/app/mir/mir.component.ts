import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-mir',
  templateUrl: './mir.component.html',
  styleUrls: ['./mir.component.scss']
})
export class MirComponent implements OnInit {
  public display_content: string = "zones";
  constructor() { }

  ngOnInit(): void {
  }


  setView(value: string): void{
    this.display_content = value;
  }

}
