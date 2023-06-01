export interface Config {
  ipUrl: string;
}
export interface Sekcja {
    sekcjaId?: number;
    nazwa: string;
    typ:number;
    pin?: number;
    inverted: boolean; // odwrocona logika na pinie
    apiOnUrl?: string;
    apiOnJson?: string;
    apiOffUrl?: string;
    apiOffJson?: string;
    mqttTopicOn?: string;
    mqttTopicState?: string;
    mqttTopicOff?: string;
    mqttOnCommand?: string;
    mqttOffCommand?: string;
  }

export interface System {
  ntpHost:String;
  ntpOffset:number;
  useNtp:number;
  lat:String;
  lon:String;
  mqttHost:String;
  mqttPort:number;
  mqttUser:String;
  mqttPwd:String;
  useMqtt:number;
  }

  export interface Program {
    nazwa:String;
    programId?: number; //id programu
    dni: String // w jakich dniach tygodnia program jest uruchamiany 0-nd,1-pn, ..6-sb
    lastProgramRun?:number; //data w sekundach od ostatnio uruchomionego programu
    aktywny:boolean; // czy program jest aktywny
    godziny:number[]; // zapis w formacie zulu hhmm
    calkowityCzasTrwaniaProgramu?:number;
  }
  export interface Sekwencja {
    programId:number;
    sekwencjaId:number;
    sekcjaId:number;
    akcja:number;
    czasTrwaniaAkcji:number; // gdy czas==0 akcja nie powoduje antyakcji
    startAkcji:number; //start akcji 
    sekwencjaLastRun?:number;
  }
  export interface StanAll {
  rtc:number;
  ntp:number;
  upT:number; //millis od restartu 
  sekcje:Stan[];
  program:{
    czasOstatniegoTestu:number;
    lastRunProgramId?:number;
    obecnaSekundaDzialaniaProgramu?:number;
    uruchomionyProgramId?:number;
    x:number;
    }   
  }
  export interface Stan {
    sekcjaId:number;
    stan:boolean;
    autoSwitchActive:number;
    lastStateChanged:number;
    timeToSwitch:number;
  }
  export interface StanSet {
    sekcjaId:number;
    stan:number;
    czas?:number;
  }
  export interface DialogData {
    data: any;
    result: any;
    
  }