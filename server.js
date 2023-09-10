const express = require("express");
const app = express();
const bodyParser = require("body-parser");

app.use(bodyParser.json()); // to support JSON-encoded bodies
app.use(
    bodyParser.urlencoded({
        // to support URL-encoded bodies
        extended: true
    })
);

// Define the REST API routes:
app.use("/api", require("./routes/index.js"));

const PORT = process.env.PORT || 5000;

app.listen(PORT, () => console.log(`Aguana server app listening on port ${PORT}!`));