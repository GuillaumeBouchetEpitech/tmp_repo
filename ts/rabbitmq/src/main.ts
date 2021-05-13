
//
//// run this
//

// docker run -d -p 15672:15672 -p 5672:5672 -p 5671:5671 --hostname my-rabbitmq --name my-rabbitmq-container rabbitmq:3-management

//
//// run this
//

import * as amqplib from "amqplib";

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

const run = async () => {

    const connection = await amqplib.connect({
        hostname: "localhost",
        port: 5672,
    });

    const queueName = "my-lovely-queue";

    { // consumer

        const channel = await connection.createChannel();
        await channel.assertQueue(queueName);

        // max concurent message(s) being processed
        // => must acknowledge message(s) before any new one(s) can be received
        channel.prefetch(2);

        channel.consume(queueName, async (msg) => {

            if (msg === null)
                return;

            await sleep(250); // for debug purpose

            console.log(msg.content.toString());
            channel.ack(msg);
        });

    } // consumer

    { // producer

        const channel = await connection.createChannel();
        await channel.assertQueue(queueName);

        for (let ii = 0; ii < 5; ++ii) {

            await sleep(500); // for debug purpose

            await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 1`));
            await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 2`));
            await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 3`));
            await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 4`));
            await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 5`));
        }

    } // producer
};
run();

