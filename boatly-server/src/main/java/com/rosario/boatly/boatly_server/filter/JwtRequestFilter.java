package com.rosario.boatly.boatly_server.filter;

import com.rosario.boatly.boatly_server.service.CustomUserDetailsService;
import com.rosario.boatly.boatly_server.service.JwtService;
import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;

@Component
public class JwtRequestFilter extends OncePerRequestFilter {

    @Autowired
    private JwtService jwtUtil;

    @Autowired
    private CustomUserDetailsService userDetailsService;

    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain)
            throws ServletException, IOException {
        // TODO Auto-generated method stub

        final String authHeader = request.getHeader("Authorization");
        String username=null;
        String jwt=null;
        if(authHeader !=null && authHeader.startsWith("Bearer ")) {
            jwt = authHeader.substring(7);//eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1c2VyMiIsImlhdCI6MTcyMzgzMzM1MiwiZXhwIjoxNzIzODY5MzUyfQ.ennZnBvjBuXAXZ2Y5m2NTUzWU4lDiR_i3J-Lprv74_0
            username=jwtUtil.extractUsername(jwt);
        }
        if(username != null && SecurityContextHolder.getContext().getAuthentication() == null) {
            UserDetails userDetails = userDetailsService.loadUserByUsername(username);
            if(jwtUtil.validateToken(jwt, userDetails.getUsername())) {
                UsernamePasswordAuthenticationToken usernamePasswordAuthenticationToken = new UsernamePasswordAuthenticationToken(
                        userDetails, null, userDetails.getAuthorities());
                usernamePasswordAuthenticationToken
                        .setDetails(new WebAuthenticationDetailsSource().buildDetails(request));
                SecurityContextHolder.getContext().setAuthentication(usernamePasswordAuthenticationToken);

            }

        }
        filterChain.doFilter(request, response);
    }

}

