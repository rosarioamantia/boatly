package com.rosario.boatly.boatly_server.configuration;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.CorsRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

@Configuration
public class CorsConfig {

    @Bean
    public WebMvcConfigurer corsConfigurer() {
        return new WebMvcConfigurer() {
            @Override
            public void addCorsMappings(CorsRegistry registry) {
                registry.addMapping("/**") // Applica CORS a tutti gli endpoint
                        .allowedOrigins("http://localhost:4200") // Sostituisci con l'URL del tuo front end
                        .allowedMethods("GET", "POST", "PUT", "DELETE", "OPTIONS") // Permetti i metodi HTTP
                        .allowedHeaders("*"); // Permetti tutti gli header
            }
        };
    }
}
