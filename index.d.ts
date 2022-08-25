//
// Created by Anton Spivak.
//
declare module "node-tonlib" {
  /**
   * 0 - none
   * 1 - fatal
   * 2 - error
   * 3 - warning
   * 4 - info
   * 5 - debug
   * 1024 - all
   */
  export type LogLevel = -1 | 0 | 1 | 2 | 3 | 4 | 1024;

  /**
   * Blockhain network
   */
  export type Network = "mainnet" | "testnet";

  /**
   * String, encoded JSON to string
   */
  export type Configuration = string;

  export interface Parameters {
    configuration: Configuration;
    keystorePath: string;
    network?: Network;
    logging?: LogLevel;
  }

  export interface Query {
    "@type": string;
    [key: string]: any;
  }

  export class Client {
    constructor(parameters: Parameters);

    /**
     * Executes asyncronous query
     * @param object - query with parameters
     */
    send(query: Query): Promise<any>;

    /**
     * Executes syncronous query
     * @param query - query with parameters
     */
    execute(query: Query): any;
  }
}
