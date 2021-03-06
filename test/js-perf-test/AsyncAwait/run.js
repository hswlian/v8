// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


d8.file.execute('../base.js');
d8.file.execute('baseline-babel-es2017.js');
d8.file.execute('baseline-naive-promises.js');
d8.file.execute('native.js');

var success = true;

function PrintResult(name, result) {
  print(name + '-AsyncAwait(Score): ' + result);
}


function PrintError(name, error) {
  PrintResult(name, error);
  success = false;
}


BenchmarkSuite.config.doWarmup = undefined;
BenchmarkSuite.config.doDeterministic = undefined;

BenchmarkSuite.RunSuites({ NotifyResult: PrintResult,
                           NotifyError: PrintError });
