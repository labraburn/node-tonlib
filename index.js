//
// Created by Anton Spivak.
//

const tonlibjson = require("./build/Release/tonlibjson.node");
const uuid = require("uuid");

class Client {
  #lib;
  #promises;
  #parameters;

  constructor(_parameters) {
    const parameters = Object.assign(
      {},
      {
        logging: 0,
        network: "mainnet",
      },
      _parameters
    );

    if (!parameters || !parameters.configuration) {
      throw new Error("Configuration required.");
    }

    if (!parameters.keystorePath) {
      throw new Error("Configuration `keystorePath` is required.");
    }

    this.#lib = new tonlibjson.Tonlib(parameters.logging, (data) => {
      this.#receive(data);
    });

    this.#promises = {};
    this.#parameters = parameters;

    this.#configurate();
  }

  send(object) {
    const extra = uuid.v4();
    object["@extra"] = extra;

    const string = JSON.stringify(object);
    if (!string) {
      throw new Error(`Can't parse JSON ${object}`);
    }

    let resolve, reject;
    const promise = new Promise((resolve_, reject_) => {
      resolve = resolve_;
      reject = reject_;
    });

    this.#promises[extra] = {
      resolve,
      reject,
    };

    this.#lib.send(string);
    return promise;
  }

  execute(object) {
    const string = JSON.stringify(object);
    if (!string) {
      throw new Error(`Can't parse JSON ${object}`);
    }
    return this.#lib.execute(string);
  }

  #configurate() {
    this.send({
      "@type": "init",
      options: {
        "@type": "options",
        config: {
          "@type": "config",
          config: this.#parameters.configuration,
          use_callbacks_for_network: false,
          blockchain_name: this.#parameters.network,
          ignore_cache: false,
        },
        keystore_type: {
          "@type": "keyStoreTypeDirectory",
          directory: this.#parameters.keystorePath,
        },
      },
    }).catch((error) => {
      this.#error(error.message);
    });
  }

  #receive(data) {
    const object = JSON.parse(data);
    if (!object) {
      return;
    }

    const extra = object["@extra"];
    if (!extra) {
      return;
    }

    const promise = this.#promises[extra];
    if (!promise) {
      return;
    }

    delete this.#promises[extra];
    delete object["@extra"];

    setInterval(() => {
      if (object["@type"] === "error") {
        const message = object.message || "Undefined error";
        promise.reject(new Error(message));
      } else {
        promise.resolve(object);
      }
    }, 1);
  }

  #error(message) {
    console.log("\x1b[31m", "[node-tonlib]:", message, "\x1b[0m");
  }
}

exports.Client = Client;
