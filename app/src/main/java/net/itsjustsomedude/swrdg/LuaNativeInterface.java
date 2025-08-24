package net.itsjustsomedude.swrdg;

import android.util.Log;

import java.lang.reflect.Method;
import java.util.ArrayList;

public class LuaNativeInterface {
	private static final String TAG = "LNI_Java";

	private static class LNIMethod {
		Class<?> clazz;
		// methodID is only in C.
		int[] params;
		int returnType;

		String methodName;

        LNIMethod(String methodName, Class<?> clazz, int[] params, int returnType) {
            this.methodName = methodName;
			this.clazz = clazz;
			this.params = params;
			this.returnType = returnType;
        }
    }

	private static final ArrayList<LNIMethod> methods = new ArrayList<>();

//	private static final HashMap<String, Class<?>> classes = new HashMap<>();
//	private static final HashMap<String, String> signatures = new HashMap<>();
//	private static final HashMap<String, int[]> params = new HashMap<>();

	private static native void nativeInit();
	private static native void register(Class<?> targetClass, String methodName, String signature, int[] params, int returnType);

	public static void init() {
		nativeInit();
		// Register all the "statically waiting" methods.

		addMethod(LNIFunctions.class, "setSpeed");
		addMethod(LNIFunctions.class, "spit");
		addMethod(LNIFunctions.class, "spitReturn");
	}

	private static String getClassSignature(Class<?> clazz) throws IllegalArgumentException {
		if(clazz == null) throw new IllegalArgumentException("Class is null");

		if(clazz.isArray()) {
			// getName already creates the right signature for array types.
			return clazz.getName();
		} else if (clazz.equals(void.class)) {
			return "V";
        } else if (clazz.isPrimitive()) {
			switch (clazz.getName()) {
				case "boolean": return "Z";
				case "byte": return "B";
				case "char": return "C";
				case "short": return "S";
				case "int": return "I";
				case "long": return "J";
				case "float": return "F";
				case "double": return "D";
				default: throw new IllegalArgumentException("Unsupported primitive type: " + clazz.getName());
			}
		} else {
			return "L" + clazz.getName().replace(".", "/") + ";";
		}
	}

	private static String getMethodSignature(Method m) {
		if(m == null) throw new IllegalArgumentException("Method is null");

		StringBuilder sig = new StringBuilder();

		sig.append("(");
		for (Class<?> param : m.getParameterTypes()) {
			sig.append(getClassSignature(param));
		}
		sig.append(")");
		sig.append(getClassSignature(m.getReturnType()));

		return sig.toString();
	}

	/**
	 * Returns true if the passed class is a valid param or return value for an LNI function.
	 */
	private static boolean validLNIClass(Class<?> clazz) {
		return clazz == String.class || clazz == void.class || clazz.isPrimitive();
	}

	private static int getLNIType(Class<?> clazz) {
		if(clazz.equals(void.class)) return 0;
		else if (clazz.equals(boolean.class)) return 1;
		else if (clazz.isPrimitive()) return 2;
		else if (clazz.equals(String.class)) return 3;

		else return -1;
	}

	public static void addMethod(Class<?> clazz, String methodName) {
		Method[] ms = clazz.getMethods();

		Method method = null;
		for (Method m : ms) {
			if(m.getName().equals(methodName)) {
				method = m;
				break;
			}
		}
		if(method == null) {
			Log.e(TAG, "Could not find LNI method " + methodName);
			return;
		}

		Class<?> returnType = method.getReturnType();
		if(!validLNIClass(returnType)) {
			Log.e(TAG, "Invalid return type '" + returnType.getName() + "' for LNI function '" + methodName + "'");
			return;
		}

		Class<?>[] paramClasses = method.getParameterTypes();
		int[] paramTypes = new int[paramClasses.length];
		int i = 0;
		for (Class<?> param : paramClasses) {
			if (!validLNIClass(param)) {
				Log.e(TAG, "Invalid parameter type '" + param.getName() + "' for LNI function '" + methodName + "'");
				return;
			}
			paramTypes[i] = getLNIType(param);
		}

		String sig = getMethodSignature(method);

//		classes.put(methodName, clazz);
//		signatures.put(methodName, sig);
//		params.put(methodName, paramTypes);

		Log.d(TAG, "Begin register... method: " + methodName + ", sig: " + sig);
		register(clazz, methodName, sig, paramTypes, getLNIType(returnType));

		Log.d(TAG, "Added new method " + methodName);
	}

//	// Called from C
//	public static Class<?> getClass(String ms) {
//		return classes.get(ms);
//	}
//
//	// Called from C
//	public static String getSignature(String ms) {
//		return signatures.get(ms);
//	}
//
//	// Called from C
//	public static int[] getParams(String ms) {
//		return params.get(ms);
//	}
}
