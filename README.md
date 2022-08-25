# node-tonlib

Installation

```
npm install node-tonlib
```

Usage

```ts
import tonlib from "node-tonlib";
import fetch from "node-fetch";

const configuration: tonlib.Configuration = await (
  await fetch("https://ton.org/global-config.json")
).text();

const client = new tonlib.Client({
  configuration: configuration,
  keystorePath: `${__dirname}/keystore`,
  network: "mainnet",
});

await this.client
  .send({
    "@type": "raw.getAccountState",
    account_address: {
      account_address: "EQCd3ASamrfErTV4K6iG5r0o3O_hl7K_9SghU0oELKF-sxDn",
    },
  })
  .then((res) => {
    console.log(res);
  });
```