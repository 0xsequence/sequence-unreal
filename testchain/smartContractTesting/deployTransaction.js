const { ethers } = require("ethers");
const fs = require('fs');
var code = fs.readFileSync("../contracts/vm-bytecode.txt").toString('utf-8');

const provider = new ethers.providers.JsonRpcProvider("http://127.0.0.1:8545/", { chainId: 1337 });

const privateKey = "abc0000000000000000000000000000000000000000000000000000000000001";
const signer = new ethers.Wallet(privateKey, provider);
const NONCE = "0x00";
const GASPRICE = "0x0003b9aca00";
const GASLIMIT = "0x00f4240";
const TO = ""
const VALUE = "";
const CHAIN_ID = 1337;
const byte_code = code;
const DATA = ethers.utils.hexlify(byte_code);
(async () => {
  // Create a contract factory
  const contractFactory = new ethers.ContractFactory([], DATA, signer);
  // Deploy the contract
  const transaction = await contractFactory.getDeployTransaction();
  transaction.nonce = NONCE;
  transaction.gasPrice = GASPRICE;
  transaction.gasLimit = GASLIMIT;
  transaction.to = TO;
  transaction.value = VALUE;
  transaction.data = DATA;
  transaction.chainId = CHAIN_ID;
  const signedTransaction = await signer.signTransaction(transaction);
  console.log(signedTransaction)
  const deployment = await provider.sendTransaction(signedTransaction);
  console.log("Contract deployed with transaction hash:", deployment.hash);
})();