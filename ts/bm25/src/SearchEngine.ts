
import * as fs from 'node:fs'

// @ts-ignore
import bm25 from 'wink-bm25-text-search';

export class SearchEngine {

  // no types...
  private _engine: any;

  constructor(getTokens: (text: string) => string[]) {
    this._engine = bm25();
    this._engine.defineConfig( { fldWeights: { body: 1 } } );
    this._engine.definePrepTasks( [ getTokens ] );
  }

  //
  //
  //

  addEntry(body: string, uuid: string) {
    this._engine.addDoc({ body }, uuid);
  }

  //
  //
  //

  exportToFile(filepath: string) {
    const toSave = this._engine.exportJSON();
    fs.writeFileSync(filepath, JSON.stringify(toSave), 'utf8');
  }

  importFromFile(filepath: string) {
    const toLoad = JSON.parse(fs.readFileSync(filepath, 'utf8'));
    this._engine.importJSON(toLoad);
  }

  //
  //
  //

  consolidate() {
    this._engine.consolidate();
  }

  //
  //
  //

  search(query: string, limit: number = 10): { uuid: string; score: string; }[] {
    return this._engine.search( query, limit ).map((val: any) => ({ uuid: val[0], score: val[1], }));
  }

}
