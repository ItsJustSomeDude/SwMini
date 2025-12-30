package net.itsjustsomedude.swrdg;

import android.util.Log;

import java.lang.reflect.Method;
import java.util.ArrayList;

public class LuaNativeInterface {
	private static final String TAG = "LNI_Java";
	private static final ArrayList<WaitingFunctionEntry> waitingMethods = new ArrayList<>();
	/**
	 * Whether the Native Init is done. If `true`, functions are ready to be sent to C.
	 */
	private static boolean initialized = false;

	/**
	 * Send a method to C to be registered. nativeInit must have been completed already.
	 */
	private static native void nativeRegister(Class<?> targetClass, String methodName, String signature, int[] params, int returnType);

	/** Performs the C-side initialization for LNI.
	 * It creates global references, sets up tables, etc. */
//    private static native void nativeInit();

	/**
	 * Start up the LNI system. This is called by the MainActivity after the engine has been started up.
	 */
	public static void init() {
		if (initialized) {
			Log.e(TAG, "Attempted to double-initialize LNI Class!");
			return;
		}

//        nativeInit();
		initialized = true;
		// Now that `initialized` is true, function registrations will be sent to native immediately.

		for (WaitingFunctionEntry method : waitingMethods) {
			performRegistration(method.clazz, method.methodName);
		}
	}

	private static String getClassSignature(Class<?> clazz) throws IllegalArgumentException {
		if (clazz == null) throw new IllegalArgumentException("Class is null");

		if (clazz.isArray()) {
			// getName already creates the right signature for array types.
			return clazz.getName();
		} else if (clazz.equals(void.class)) {
			return "V";
		} else if (clazz.isPrimitive()) {
			switch (clazz.getName()) {
				case "boolean":
					return "Z";
				case "byte":
					return "B";
				case "char":
					return "C";
				case "short":
					return "S";
				case "int":
					return "I";
				case "long":
					return "J";
				case "float":
					return "F";
				case "double":
					return "D";
				default:
					throw new IllegalArgumentException("Unsupported primitive type: " + clazz.getName());
			}
		} else {
			return "L" + clazz.getName().replace(".", "/") + ";";
		}
	}

	private static String getMethodSignature(Method m) {
		if (m == null) throw new IllegalArgumentException("Method is null");

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
	 * Allowed: void, bool, double, String
	 */
	private static boolean validLNIClass(Class<?> clazz) {
		return clazz == void.class || clazz == boolean.class || clazz == double.class || clazz == String.class;
	}

	private static int getLNIType(Class<?> clazz) {
		if (clazz.equals(void.class)) return 0;
		else if (clazz.equals(boolean.class)) return 1;
		else if (clazz.equals(double.class)) return 2;
		else if (clazz.equals(String.class)) return 3;

		else return -1;
	}

	public static void registerMethod(Class<?> clazz, String methodName) {
		// This is what external functions use to register a method.
		if (initialized) {
			Log.i(TAG, "Doing now " + methodName);
			// We've already done the C parts, so it's ready to receive a method.
			performRegistration(clazz, methodName);
		} else {
			Log.i(TAG, "Doing later " + methodName);
			// Add the method to the queue list.
			waitingMethods.add(new WaitingFunctionEntry(clazz, methodName));
		}
	}

	/**
	 * Must be called after nativeInit is done.
	 */
	private static void performRegistration(Class<?> clazz, String methodName) {
		Method[] ms = clazz.getMethods();

		// TODO: Make this work with Overloads

		Method method = null;
		for (Method m : ms) {
			if (m.getName().equals(methodName)) {
				method = m;
				break;
			}
		}
		if (method == null) {
			Log.e(TAG, "Could not find LNI method " + methodName + " in class " + clazz.getName());
			return;
		}

		Class<?> returnType = method.getReturnType();
		if (!validLNIClass(returnType)) {
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
			i++;
		}

		String sig = getMethodSignature(method);

		Log.d(TAG, "Begin register... method: " + methodName + ", sig: " + sig);
		// Send it over to C!
		nativeRegister(clazz, methodName, sig, paramTypes, getLNIType(returnType));

		Log.d(TAG, "Added new method " + methodName);
	}

	private static class WaitingFunctionEntry {
		Class<?> clazz;
		String methodName;

		WaitingFunctionEntry(Class<?> clazz, String methodName) {
			this.clazz = clazz;
			this.methodName = methodName;
		}
	}
}
