
#include "BCC.h"
#include "BIPLAN.h"

BCC compiler;
BIPLAN_Interpreter interpreter;

bool error = false;

void error_callback(char *position, const char *string) {
  Serial.print("error: ");
  Serial.print(string);
  if(position) {
    Serial.print(" ");
    Serial.print(*position);
    Serial.print(" at position ");
    Serial.print(position - interpreter.program_start);
  }
  Serial.println();
  error = true;
};

char program[] =
"print \"\nBIPLAN CR.1 prime calculator \nDigit the test range: \" \n\
$index = 0 \n\
$range = 0 \n\
$result = 0 \n\
while true \n\
  if serialAvailable \n\
    $value = serialRead \n\
    if $value < 1 continue endif \n\
    if $value == 13 break endif \n\
    if ($value < 48) || ($value > 57) \n\
      print \"Only numbers are accepted \n\" \n\
      restart \n\
    endif \n\
    :str[$index++] = $value \n\
  endif \n\
next \n\
print :str, \"\n\" \n\
$range = number :str \n\
for $i = 0 to $range \n\
  if prime($i) \n\
    print $i, \"\n\" \n\
    ++$result \n\
  endif \n\
next \n\
$time = millis \n\
for $i = 0 to $range prime($i) next \n\
print \"Elapsed time: \", millis - $time, \" milliseconds \n\" \n\
print \"Prime numbers found: \", $result, \"\n\" \n\
restart \n\
function prime($n) \n\
  if $n <= 1 return 0 endif \n\
  if $n <= 3 return 1 endif \n\
  if ($n % 2 == 0) || ($n % 3 == 0) return 0 endif \n\
  for $u = 2 to sqrt($n) + 1 \n\
    if($n % $u == 0) return 0 endif \n\
    if $n == $u return 1 endif \n\
  next \n\
return 1 \n\
\n";

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  // Show human readable input program
  Serial.print("\nBIPLAN human-readable source:\n\n");
  Serial.print(program);
  uint16_t length = strlen(program);
  // Print stats
  Serial.println("\n--------------------------");
  Serial.print("Program length: ");
  Serial.print(length);
  Serial.println(" bytes");
  uint32_t time = millis();
  // Compile
  Serial.print("\nBCC compilation result:\n\n");
  compiler.error_callback = error_callback;
  compiler.run(program);
  // Print compiled source
  Serial.print("\nBIPLAN machine language:\n\n");
  Serial.print(program);
  uint16_t new_length = strlen(program);
  // Print Stats
  Serial.println("\n\n--------------------------");
  Serial.print("Compilation duration: ");
  Serial.print(millis() - time);
  Serial.println(" milliseconds");
  Serial.print("BIPLAN program length: ");
  Serial.print(length);
  Serial.println(" bytes");
  Serial.print("BIPLAN compiled program length: ");
  Serial.print(new_length);
  Serial.println(" bytes");
  Serial.print("Reduction: ");
  Serial.print(100 - (new_length * 100.0) / length);
  Serial.println("%");
  Serial.println("Program output:");
  Serial.println();
  // Initialize interpreter
  interpreter.initialize(
    program,
    error_callback,
    &Serial,
    &Serial,
    &Serial
  );
  // Check for compilation errors
  if(error) {
    interpreter.ended = true;
    Serial.println("Fix your code and retry.");
  }
};

void loop() {
  while(!interpreter.ended)
    interpreter.run();
  while(true);
};
