
//
//// run this
//

// install and run
//     docker run -d -p 15672:15672 -p 5672:5672 -p 5671:5671 --hostname my-rabbitmq --name my-rabbitmq-container rabbitmq:3-management
// run
//     docker run -d -p 15672:15672 -p 5672:5672 -p 5671:5671 --hostname my-rabbitmq rabbitmq:3-management

//
//// run this
//

import * as amqplib from "amqplib";

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

const run = async () => {

    const connection = await amqplib.connect({ hostname: "localhost", port: 5672 });

    const producerConsumerTest = async () => {

        console.log();
        console.log("producerConsumerTest");
        console.log();

        const queueName = "my-lovely-queue";

        { // consumer

            for (let ii = 0; ii < 5; ++ii) {

                const name = `consumer[${ii}]`;

                const channel = await connection.createChannel();

                await channel.assertQueue(queueName, { durable: true });

                // max concurent message(s) being processed
                // => must acknowledge message(s) before any new one(s) can be received
                channel.prefetch(2);

                channel.consume(queueName, async (msg) => {

                    if (msg === null)
                        return;

                    await sleep(250); // for debug purpose

                    console.log(name, msg.content.toString());

                    channel.ack(msg);
                });
            }

        } // consumer

        { // producer

            const channel = await connection.createChannel();

            await channel.assertQueue(queueName, { durable: true });

            for (let ii = 0; ii < 10; ++ii) {

                await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 1`), { persistent: true });
                await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 2`), { persistent: true });
                await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 3`), { persistent: true });
                await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 4`), { persistent: true });
                await channel.sendToQueue(queueName, Buffer.from(`message ${ii} 5`), { persistent: true });
            }

            console.log("producer complete");

        } // producer

        await sleep(2000); // for debug purpose
    };

    const broadcastTest = async () => {

        console.log();
        console.log("broadcastTest");
        console.log();

        const exchangeName = "broadcast_test";

        { // consumer

            for (let ii = 0; ii < 3; ++ii) {

                const name = `consumer[${ii}]`;

                const channel = await connection.createChannel();

                await channel.assertExchange(exchangeName, "fanout", { durable: false });

                const response = await channel.assertQueue("", { exclusive: true });

                channel.bindQueue(response.queue, exchangeName, '');

                // max concurent message(s) being processed
                // => must acknowledge message(s) before any new one(s) can be received
                channel.prefetch(2);

                channel.consume(response.queue, async (msg) => {

                    if (!msg)
                        return;

                    await sleep(250); // for debug purpose

                    console.log(name, msg.content.toString());

                    channel.ack(msg);

                }/*, { noAck: true }*/);
            }

        } // consumer

        { // producer

            const channel = await connection.createChannel();

            await channel.assertExchange(exchangeName, "fanout", { durable: false });

            for (let ii = 0; ii < 5; ++ii)
                await channel.publish(exchangeName, "", Buffer.from(`message ${ii}`));

            console.log("producer complete");

        } // producer

        await sleep(2000); // for debug purpose
    };

    const routingTest = async () => {

        console.log();
        console.log("routingTest");
        console.log();

        const exchangeName = "routing_test";

        const allGroups = [
            [ "value_a", "value_b", "value_c" ],
            [ "value_c" ],
        ];

        { // consumer

            for (let ii = 0; ii < allGroups.length; ++ii) {

                const name = `consumer[${allGroups[ii].join("-")}]`;

                console.log("start consumer", name);

                const channel = await connection.createChannel();

                await channel.assertExchange(exchangeName, "direct", { durable: false });

                const response = await channel.assertQueue("", { exclusive: true });

                for (let jj = 0; jj < allGroups[ii].length; ++jj)
                    await channel.bindQueue(response.queue, exchangeName, allGroups[ii][jj]);

                // max concurent message(s) being processed
                // => must acknowledge message(s) before any new one(s) can be received
                channel.prefetch(2);

                channel.consume(response.queue, async (msg) => {

                    if (!msg)
                        return;

                    await sleep(250); // for debug purpose

                    console.log(name, msg.fields.routingKey, msg.content.toString());

                    channel.ack(msg);

                }/*, { noAck: true }*/);
            }

        } // consumer

        { // producer

            console.log("start producer");

            const allUniqueGroups = new Set([ ...allGroups[0], ...allGroups[1] ]);

            const channel = await connection.createChannel();

            await channel.assertExchange(exchangeName, "direct", { durable: false });

            for (let ii = 0; ii < 5; ++ii) {

                allUniqueGroups.forEach(async (group) => {

                    await channel.publish(exchangeName, group, Buffer.from(`message ${ii}`));
                });
            }

            console.log("producer complete");

        } // producer

        await sleep(2000); // for debug purpose
    };

    const topicTest = async () => {

        console.log();
        console.log("routingTest");
        console.log();

        const exchangeName = "topic_logs";

        // const allGroups = [
        //     [ "value_a", "value_b", "value_c" ],
        //     [ "value_c" ],
        // ];

        const allTopics = [
            [ "testA.#", "#.testB" ],
            [ "testC.*" ],
            [ "*.testC" ],
        ];

        { // consumer

            for (let ii = 0; ii < allTopics.length; ++ii) {

                const name = `consumer[${allTopics[ii].join("-")}]`;

                console.log("start consumer", name);

                const channel = await connection.createChannel();

                await channel.assertExchange(exchangeName, "topic", { durable: false });

                const response = await channel.assertQueue("", { exclusive: true });

                for (let jj = 0; jj < allTopics[ii].length; ++jj)
                    await channel.bindQueue(response.queue, exchangeName, allTopics[ii][jj]);

                // max concurent message(s) being processed
                // => must acknowledge message(s) before any new one(s) can be received
                channel.prefetch(2);

                channel.consume(response.queue, async (msg) => {

                    if (!msg)
                        return;

                    await sleep(250); // for debug purpose

                    console.log(name, msg.fields.routingKey, msg.content.toString());

                    channel.ack(msg);

                }/*, { noAck: true }*/);
            }

        } // consumer

        { // producer

            console.log("start producer");

            // const allUniqueGroups = new Set([ ...allGroups[0], ...allGroups[1] ]);

            const channel = await connection.createChannel();

            await channel.assertExchange(exchangeName, "topic", { durable: false });

            const keys = [
                "testA.testB",
                "testA.testC",
                "testC.testC",
                "testC.testA",
            ];

            let index = 0;
            for (const key of keys) {
                await channel.publish(exchangeName, key, Buffer.from(`message ${index++}`));
            }


            console.log("producer complete");

        } // producer

        await sleep(2000); // for debug purpose
    };

    await producerConsumerTest();
    await broadcastTest();
    await routingTest();
    await topicTest();

    process.exit(0);

};
run();
