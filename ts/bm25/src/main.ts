
import * as fs from 'node:fs';
import * as path from 'node:path';
import * as readline from 'node:readline';

// @ts-ignore
import nodeLefff from 'node-lefff'; // no types

// @ts-ignore
import Stemmer from 'natural/lib/natural/stemmers/stemmer_fr'; // no types

import TurndownService from 'turndown'; // htmlToMarkdown

import { SearchEngine } from './SearchEngine'; // search engine

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

const folderPath = "TODO";

const dataFolderPath = path.join(__dirname, '../.cached-data');
const outputFilepath = path.join(dataFolderPath, 'all_rows.json');
const cachedSearchEngineDataFilepath = path.join(dataFolderPath, 'cached-search-engine-data.json');

interface IRowData {
  id: string;
  description: string;
  number: string;
  title: string;
  date: string;
  legislatureId: string;
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

const _preProcessAndSaveAsJsonFile = (): {
  allRows: IRowData[],
  rowsMap: Map<string, IRowData>,
} => {

  const allRows: IRowData[] = [];
  const rowsMap = new Map<string, IRowData>();

  if (!fs.existsSync(outputFilepath)) {

    console.log('json file not found, will pre-process');

    const htmlToMarkdown = new TurndownService();

    const allDirEntries = fs.readdirSync(folderPath, { recursive: true, withFileTypes: true });

    console.log('allDirEntries', allDirEntries.length)

    let totalSuccess = 0;

    allDirEntries
      .filter(val => val.isFile())
      // .slice(0,5) // debug
      .forEach((entry, index) => {

        if (index > 0 && (index % 1000) === 0) {
          console.log(`pre-process, progress: ${index}/${allDirEntries.length} (${((index/allDirEntries.length) * 100).toFixed(1)}%)`)
        }

        const filepath = path.join(entry.parentPath, entry.name);

        const textContent = fs.readFileSync(filepath, 'utf8');
        const jsonValues = JSON.parse(textContent);

        let exposeSommaire = jsonValues?.amendement?.corps?.contenuAuteur?.exposeSommaire;

        if (!exposeSommaire) {
          return;
        }

        totalSuccess += 1;

        const markdown = htmlToMarkdown.turndown(exposeSommaire);

        const am = jsonValues.amendement;
        const newRow: IRowData = {
          id: am.uid,
          description: markdown,
          number: am.identification.numeroLong,
          title: am.identification.numeroLong,
          date: (
            typeof am.cycleDeVie.dateDepot === 'string'
              ? new Date(am.cycleDeVie.dateDepot)
              : typeof am.cycleDeVie.datePublication === 'string'
                ? new Date(am.cycleDeVie.datePublication)
                : new Date()
          ).toISOString(),
          legislatureId: am.legislature,
        };

        allRows.push(newRow);
        rowsMap.set(newRow.id, newRow);
      });


    console.log('allDirEntries', allDirEntries.length)
    console.log('totalSuccess', totalSuccess)

    fs.mkdirSync(dataFolderPath, { recursive: true });
    fs.writeFileSync(outputFilepath, JSON.stringify(allRows, null, 2), 'utf8');

    console.log('pre-processed json saved');
  }
  else {
    console.log('pre-processed json file found, loading');

    const tmpRows = JSON.parse(fs.readFileSync(outputFilepath, 'utf8'));
    tmpRows.forEach((currRow: any) => {
      allRows.push(currRow);
      rowsMap.set(currRow.id, currRow);
    });

    console.log('pre-processed json file loaded');
  }

  return {
    allRows,
    rowsMap,
  };
}

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

const _initializeSearchEngine = async (allRows: IRowData[]): Promise<SearchEngine> => {

  console.log('initializing the search engine');

  const nl = await nodeLefff.load();
  const LefffLemmer = new Stemmer();
  LefffLemmer.stem = nl.lem;

  const _getTokens = (text: string): string[] => {
    return LefffLemmer.tokenizeAndStem(text);
  };
  const searchEngine = new SearchEngine(_getTokens);


  if (!fs.existsSync(cachedSearchEngineDataFilepath)) {
    console.log(`cached searchEngine data not found: loading`);

    // start ingesting the rows
    allRows.forEach((currRow, index) => {

      if (index > 0 && (index % 1000) === 0) {
        console.log(`add-entry, progress: ${index}/${allRows.length} (${((index/allRows.length) * 100).toFixed(1)}%)`)
      }

      // ingest one document
      searchEngine.addEntry(currRow.description, currRow.id);
    });

    console.log(`searchEngine loaded`);

    // export+cache so that next launch is faster

    console.log(`saving searchEngine data to file`);

    fs.mkdirSync(dataFolderPath, { recursive: true });
    searchEngine.exportToFile(cachedSearchEngineDataFilepath);

    console.log(`searchEngine data saved`);
  }
  else {
    // import previously ingested doc

    console.log(`cached searchEngine data file found`);

    searchEngine.importFromFile(cachedSearchEngineDataFilepath);

    console.log(`cached searchEngine data loaded`);
  }

  console.log(`searchEngine consolidate: start`);

  // /!\ documents CANNOT be ingested after that call
  searchEngine.consolidate();

  console.log(`searchEngine consolidate: done`);

  return searchEngine;
}

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

const _myAsyncInput = async (prompt: string) => {
  return new Promise<string>((resolve) => {

    const myInterface = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      terminal: true,
    });

    myInterface.question(prompt, (userInput) => {
      myInterface.close();
      resolve(userInput);
    });
  });
};

//
//
//
//
//

//
//
//
//
//

//
//
//
//
//

const asyncRun = async () => {

  const { allRows, rowsMap } = _preProcessAndSaveAsJsonFile();

  const searchEngine = await _initializeSearchEngine(allRows);

  //
  //
  //

  do {

    console.log('###########################################################')

    const query = await _myAsyncInput('ask stuff: ')

    console.log(`the query is: "${query}"`);

    const limit = 5;
    const results = searchEngine.search(query, limit);

    console.log(`results.length: ${results.length} (max: ${limit})`);
    results.forEach((currResult, index) => {
      console.log(` -> result[${index}] | uuid: ${currResult.uuid}`);
      console.log(` ->                  | score: ${currResult.score}`);
    });

    //
    //
    //

    results.forEach((currResult, index) => {
      console.log(`===== ===== ${index} ===== =====`);

      const currRow = rowsMap.get(currResult.uuid);
      if (!currRow) {
        throw new Error("row not found by idea");
      }

      console.log(` -> (score):       ${currResult.score}`);
      console.log(`    id:            ${currRow.id}`);
      console.log(`    title:         ${currRow.title}`);
      console.log(`    number:        ${currRow.number}`);
      console.log(`    date:          ${currRow.date}`);
      console.log(`    legislatureId: ${currRow.legislatureId}`);
      console.log(` -- description --`);
      console.log(currRow.description);
      console.log(` -- /description --`);
    });
  }
  while (true);

};
asyncRun();

