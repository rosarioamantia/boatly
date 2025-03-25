import { Boat } from "./boat";
import { User } from "./user";

export interface DetectedStolen {
    id: string;
    boat: Boat;
    time: Date;
}