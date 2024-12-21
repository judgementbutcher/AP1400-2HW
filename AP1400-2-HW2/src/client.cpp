#include "client.h"
#include "server.h"

std::string Client::get_id() {
    return id;
}

std::string Client::get_publickey() {
    return public_key;
}

double Client::get_wallet() {
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) {

}

bool Client::transfer_money(std::string receiver, double value) {

}

size_t Client::generate_nonce() {

}

