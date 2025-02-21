package com.rosario.boatly.boatly_server.model;


import jakarta.persistence.*;

import java.time.LocalDateTime;

@Entity
@Table(name = "boat")
public class Boat {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    @Column(name = "id")
    private String id;
    private String name;
    private boolean stolen;
    private LocalDateTime lastUpdate;
    public Boat() {
    }

    public Boat(String id, String name, boolean stolen, LocalDateTime lastUpdate) {
        this.id = id;
        this.name = name;
        this.stolen = stolen;
        this.lastUpdate = lastUpdate;
    }

    // Getter e Setter
    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean isStolen() {
        return stolen;
    }

    public void setStolen(boolean stolen) {
        this.stolen = stolen;
    }

    public LocalDateTime getLastUpdate() {
        return lastUpdate;
    }

    public void setLastUpdate(LocalDateTime lastUpdate) {
        this.lastUpdate = lastUpdate;
    }

    // toString per debug e logging
    @Override
    public String toString() {
        return "Boat{" +
                "id='" + id + '\'' +
                ", name='" + name + '\'' +
                ", stolen=" + stolen +
                ", lastUpdate=" + lastUpdate +
                '}';
    }
}
