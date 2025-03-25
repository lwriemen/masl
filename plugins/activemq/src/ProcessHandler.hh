#ifndef ActiveMQ_ProcessHandler_HH
#define ActiveMQ_ProcessHandler_HH

#include "idm/ProcessHandler.hh"

#include "amqp_asio/condition_var.hh"
#include "amqp_asio/connection.hh"
#include "amqp_asio/session.hh"

#include "logging/log.hh"

namespace InterDomainMessaging {

    namespace ActiveMQ {

        class ProcessHandler : public InterDomainMessaging::ProcessHandler {
          public:
            ProcessHandler();
            std::unique_ptr<InterDomainMessaging::Consumer> createConsumer(std::string topic) override;
            std::unique_ptr<InterDomainMessaging::Producer> createProducer(std::string topic) override;

            amqp_asio::Session getSession() {
                return session;
            }

            asio::awaitable<void> isInitialised() {
                return initialisedCond.wait([this] {
                    return initialised;
                });
            }

            static ProcessHandler &getInstance();

          private:
            xtuml::logging::Logger log;
            amqp_asio::Connection conn;
            amqp_asio::Session session;
            bool initialised;
            amqp_asio::ConditionVar initialisedCond;
        };

    } // namespace ActiveMQ

} // namespace InterDomainMessaging

#endif
