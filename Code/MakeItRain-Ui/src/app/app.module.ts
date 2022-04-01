import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

// Supporting Modules
import { NgxMasonryModule } from 'ngx-masonry';

// Materials Components
import { MatToolbarModule} from '@angular/material/toolbar';
import { MatIconModule} from '@angular/material/icon';
import { MatCardModule } from '@angular/material/card';
import { MatSidenavModule} from '@angular/material/sidenav';
import { MatButtonModule} from '@angular/material/button';
import { MatTabsModule} from '@angular/material/tabs';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatTableModule } from '@angular/material/table';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatDialogModule } from '@angular/material/dialog';

//Make it Rain Components
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
    NgxMasonryModule,
    MatToolbarModule,
    MatIconModule,
    MatCardModule,
    MatButtonModule,
    MatSidenavModule,
    MatCheckboxModule,
    MatInputModule,
    MatTableModule,
    MatFormFieldModule,
    MatTabsModule,
    MatDialogModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
