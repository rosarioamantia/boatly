package com.rosario.boatly.boatly_server.configuration;

import com.rosario.boatly.boatly_server.service.MQTTService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.integration.annotation.IntegrationComponentScan;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.MessagingException;

@IntegrationComponentScan
@Configuration
public class InboundMQTTConfig {
    // chiamare InboundMQTTConfig

    @Autowired
    MQTTService mqttService;

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    /*
    @Bean
    public MessageChannel mqttInputChannel2() {
        return new DirectChannel();
    }

     */

    @Bean
    public MessageProducer inbound() {
        MqttPahoMessageDrivenChannelAdapter adapter =
                new MqttPahoMessageDrivenChannelAdapter("tcp://broker.emqx.io:1883", "testClient",
                        "test/receiver");
        adapter.setCompletionTimeout(15000);
        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setQos(2);
        adapter.setOutputChannel(mqttInputChannel());
        return adapter;
    }

    /*
    @Bean
    public MessageProducer inbound2() {
        MqttPahoMessageDrivenChannelAdapter adapter =
                new MqttPahoMessageDrivenChannelAdapter("tcp://broker.hivemq.com:1883", "testClient2",
                        "boatly/presenc");
        adapter.setCompletionTimeout(15000);
        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setQos(2);
        adapter.setOutputChannel(mqttInputChannel2());
        return adapter;
    }

     */

    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler handler() {
        return new MessageHandler() {
            @Override
            public void handleMessage(Message<?> message) throws MessagingException {
                System.out.println("OKOK " + message.getPayload());
                mqttService.parseMessage(message.getPayload().toString());
            }
        };
    }

    /*
    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel2")
    public MessageHandler handler2() {
        return new MessageHandler() {
            @Override
            public void handleMessage(Message<?> message) throws MessagingException {
                System.out.println((String) message.getPayload());
                System.out.println( ( message.getPayload()).getClass());
            }

        };
    }

     */
}
