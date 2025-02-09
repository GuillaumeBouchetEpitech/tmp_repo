
import Ajv from "ajv";

const ajv = new Ajv({ allErrors: true });

const schema = {
    type: "object",
    properties: {
        foo: {type: "integer"},
        bar: {type: "string"}
    },
    required: [
        "bar"
    ],
    additionalProperties: false,
};

try {
    // throw new Error2("test");

    const validate = ajv.compile(schema);
}
catch (err) {

    // return 400 Bad Request with err.message
    console.log('400: Bad Request - ', err);

    const errors = err.message.split(",").map(msg => msg.trim());
    console.log({ errors });
}