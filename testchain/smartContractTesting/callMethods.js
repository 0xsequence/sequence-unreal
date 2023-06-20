const ethers = require('ethers');
const fs = require('fs');

const CONTRACT_ADDRESS = '0x78cfa0a45e279bfe8add6aeae31d9993ec08b66b'; //replace with address displayed in testchain when contract deployed
const PRIVATE_KEY = "abc0000000000000000000000000000000000000000000000000000000000001";
const abi = JSON.parse(fs.readFileSync('../contracts/vm-abi.json', 'utf8'));

async function main() {
  const provider = new ethers.providers.JsonRpcProvider("http://127.0.0.1:8545/", { chainId: 1337 });
  let signer = new ethers.Wallet(PRIVATE_KEY, provider);
  const new_contract = new ethers.Contract(CONTRACT_ADDRESS, abi, signer);
  const options = { value: ethers.utils.parseEther("3.0") }
  let tx = await new_contract.purchase(3, options);
  await tx.wait();
  console.log("Here is purchase transaction:", tx)
  tx = await new_contract.cupcakeBalances("0x78cfa0a45e279bfe8add6aeae31d9993ec08b66b");
  console.log("Here is balance check transaction:", tx)
  //await tx.wait();
}

main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });