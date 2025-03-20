import { User } from "./user";

export interface Boat {
    id: string;
    name: string;
    stolen: boolean;
    lastUpdate: Date;
    inHarbor: boolean;
    user: User;
}