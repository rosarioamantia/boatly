import { Boat } from "./boat";
import { User } from "./user";

export interface Trip {
    id: string;
    boat: Boat;
    date: Date;
}