
import { Sequelize, DataTypes, Transaction } from 'sequelize';



// docker run -e POSTGRES_PASSWORD=mysecretpassword -p 5432:5432 postgres:14

const sequelize = new Sequelize({
  dialect: 'postgres',
  host: 'localhost',
  port: 5432,
  database: 'postgres',
  username: 'postgres',
  password: 'mysecretpassword',
  logging: false,
});





const User = sequelize.define(
  'User',
  {
    id: {
      type: DataTypes.INTEGER,
      autoIncrement: true,
      primaryKey: true,
    },
    name: {
      type: DataTypes.STRING,
      allowNull: false,
    },
    counter: {
      type: DataTypes.INTEGER,
      defaultValue: 0,
    },
  },
  {
    timestamps: true
  },
);







const asyncMain = async () => {

  await User.sync({force: true})

  //
  //
  //

  // I only needed 1 user really.....
  for (let ii = 0; ii < 4; ++ii) {
    const user = await User.create({ name: `hello-${ii}` });
    await user.save();
  }

  //
  //
  //

  // {
  //   const allUsers = await User.findAll();

  //   console.log('allUsers', allUsers.map(val => val.dataValues));
  // }

  //
  //
  //

  {
    const allRequests: Promise<void>[] = [];
    for (let ii = 0; ii < 5; ++ii) {

      // define the callback and call it right away
      // -> only push the resulting promise
      allRequests.push((async (index: number) => {

        console.log(`async request ${index} - start`);

        const user0 = await User.findOne({ where: { name: 'hello-0' } });
        if (!user0) {
          throw new Error("LOL?");
        }

        await user0.update({
          // increment the 'counter' value of 1
          // -> all 10 callbacks are going to run in parallel
          // -> this WILL create a "race condition"
          // -> all 10 requests will try to increment the counter a the same time
          counter: user0.dataValues.counter + 1
        });

        console.log(`async request ${index} - stop`);

      })(ii));
    }

    await Promise.all(allRequests);
  }

  //
  //
  //

  {
    const user0 = await User.findOne({ where: { name: 'hello-0' } });
    if (!user0) {
      throw new Error("LOL?");
    }

    console.log(`counter value of user0 (SHOULD BE 5, WILL NOT BE 5) -> ${user0.dataValues.counter}`)
    console.log(`counter value of user0 (SHOULD BE 5, WILL NOT BE 5) -> ${user0.dataValues.counter}`)
    console.log(`counter value of user0 (SHOULD BE 5, WILL NOT BE 5) -> ${user0.dataValues.counter}`)


    // reset the value
    await user0.update({ counter: 0 });

    console.log(`counter value of user0 was reset to 0`)
  }

  //
  //
  //

  {
    const allRequests: Promise<void>[] = [];
    for (let ii = 0; ii < 5; ++ii) {

      // define the callback and call it right away
      // -> only push the resulting promise
      allRequests.push((async (index: number) => {

        // Managed transaction:
        await sequelize.transaction(async (t) => {

          console.log(`async request ${index} - start`);

          const user0 = await User.findOne({
            where: { name: 'hello-0' },
            transaction: t,
            lock: true, // <- will lock out any other transaction trying to read the same record
          });
          if (!user0) {
            throw new Error("LOL?");
          }

          await user0.update({
            // increment the 'counter' value of 1
            counter: user0.dataValues.counter + 1
          }, {
            transaction: t
          });

          console.log(`async request ${index} - stop`);

        });

      })(ii));
    }

    await Promise.all(allRequests);
  }

  //
  //
  //

  {
    const user0 = await User.findOne({ where: { name: 'hello-0' } });
    if (!user0) {
      throw new Error("LOL?");
    }

    console.log(`counter value of user0 (SHOULD BE 5, WILL BE 5) -> ${user0.dataValues.counter}`)
    console.log(`counter value of user0 (SHOULD BE 5, WILL BE 5) -> ${user0.dataValues.counter}`)
    console.log(`counter value of user0 (SHOULD BE 5, WILL BE 5) -> ${user0.dataValues.counter}`)
  }

  //
  //
  //

};
asyncMain();

