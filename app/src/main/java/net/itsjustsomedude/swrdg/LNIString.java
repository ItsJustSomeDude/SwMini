package net.itsjustsomedude.swrdg;

import android.util.Log;

import java.util.ArrayList;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class LNIString {
    private static final String TAG = "LNIString";

    public static <T> void removeTrailingNulls(ArrayList<T> list) {
        int i = list.size() - 1;
        while (i >= 0 && list.get(i) == null) {
            list.remove(i);
            i--;
        }
    }

    public static Pattern functionPattern = Pattern.compile("(\\w+)\\((.*?)\\);$");

    public static final Map<Character, Character> escapeMap = Map.of(
            'n', '\n',
            't', '\t',
            'r', '\r',
            'b', '\b',
            'f', '\f'
    );

    public static void execute(String command) {
//		Debug.Log("Beginning LNI command! (internal)");

        Matcher funcMatch = functionPattern.matcher(command);
        // Bad match, not valid LNI.
        if (!funcMatch.find()) {
            Log.e(TAG, "Bad func name");
            return;
        }

        String funcName = funcMatch.group(1);
        String paramsString = funcMatch.group(2);

        // Bad LNI instruction
        if(funcName == null || paramsString == null) {
            Log.e(TAG, "Bad regex");
            return;
        }

        // Lowercase the first letter, because Swordigo uselessly capitalizes stuff
        // so modders do the same...
        funcName = funcName.substring(0, 1).toLowerCase() + funcName.substring(1);

        // System.out.println("Function Name: " + funcName);

        ArrayList<String> stringArgs = new ArrayList<>();
        ArrayList<Double> numArgs = new ArrayList<>();

        StringBuilder arg = null;

        // Start by reading the function name.
        // boolean inName = true;
        // String funcName;
        // TODO: remove the regex, make the parser also slurp the function name.

        boolean inString = false;
        boolean inSingleString = false;
        boolean inEscape = false;
        boolean inNumber = false;
        boolean decimal = false;
        for (int i = 0; i <= paramsString.length(); i++) {
            char c = (i < paramsString.length()) ? paramsString.charAt(i) : ',';

            if(inString && !inEscape && c == '\\') {
                // "test\n"
                //      ^
                inEscape = true;
            } else if (inString && inEscape) {
                // "test\n"
                //       ^
                if(escapeMap.containsKey(c))
                    arg.append(escapeMap.get(c));
                else
                    arg.append(c);
                inEscape = false;
            } else if (inString && ((!inSingleString && c == '"') || (inSingleString && c == '\''))) {
                // "Testing"
                //         ^
                // 'smol quotes'
                //             ^
                inString = false;
                inSingleString = false;
                stringArgs.add(arg.toString());
                numArgs.add(null);
            } else if (inString) {
                // "Whatever"
                //    ^
                arg.append(c);
            } else if (inNumber && Character.isDigit(c)) {
                // 12345
                //  ^
                arg.append(c);
            } else if (inNumber && c == '.' && !decimal) {
                // 123.45
                //    ^
                arg.append('.');
                decimal = true;
            } else if (inNumber && (c == ',' || c == ' ')) {
                // 123.45,
                //       ^
                // 1234 ,
                //     ^
                inNumber = false;
                String a = arg.toString();
                stringArgs.add(a);
                numArgs.add(Double.parseDouble(a));
            } else if (/* !inString && */ !inNumber) {
                if(c == '\''){
                    inString = true;
                    inSingleString = true;
                    arg = new StringBuilder();
                }
                else if(c == '"'){
                    inString = true;
                    arg = new StringBuilder();
                }
                else if(Character.isDigit(c) || c == '.') {
                    inNumber = true;
                    arg = new StringBuilder();
                    // We have to add the first char already.
                    arg.append(c);
                }
            } else {
                // throw new Exception("Didn't expect char: " + c);
                return;
            }
        }

        removeTrailingNulls(stringArgs);
        removeTrailingNulls(numArgs);

        switch (funcName) {
            case "copy":
            case "copyToClipboard":
                // wrong number of args.
                if (stringArgs.isEmpty() || stringArgs.size() > 2) {
                    Log.e(TAG, "bad args");
                    return;
                }
                else if (stringArgs.size() == 1)
                    LNIFunctions.copyToClipboard(null, stringArgs.get(0));
                else
                    LNIFunctions.copyToClipboard( stringArgs.get(0), stringArgs.get(1));
                break;
            case "openURL":
            case "openUrl":
                // System.out.println(stringArgs.get(0));
                LNIFunctions.openUrl(stringArgs.get(0), false);
                break;
            case "setSpeed":
                // System.out.println(numArgs.get(0));
                // TODO:
                LNIFunctions.setSpeed(numArgs.get(0));
                break;
            case "quit":
                LNIFunctions.quit();
                break;

            default:
                Log.e(TAG, "Unknown LNI Func " + funcName);
        }
    }

}
