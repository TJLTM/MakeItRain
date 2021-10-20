import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MatIconModule} from '@angular/material/icon';
import {MatSidenavModule} from '@angular/material/sidenav';
import {MatButtonModule} from '@angular/material/button';
import {MatTabsModule} from '@angular/material/tabs';


import { MirComponent } from './mir/mir.component';
import { ZonesComponent } from './mir/selections/zones/zones.component';
import { SettingsComponent } from './mir/selections/settings/settings.component';
import { ZoneInfoComponent } from './mir/selections/zones/zone-info/zone-info.component';


@NgModule({
  declarations: [
    AppComponent,
    MirComponent,
    ZonesComponent,
    SettingsComponent,
    ZoneInfoComponent
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    MatToolbarModule,
    MatIconModule,
    MatButtonModule,
    MatSidenavModule,
    MatTabsModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
