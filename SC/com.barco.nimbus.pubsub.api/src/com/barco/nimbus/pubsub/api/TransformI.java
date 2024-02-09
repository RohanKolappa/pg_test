package com.barco.nimbus.pubsub.api;

/**
 * A service that is capable of both filtering and extracting the interesting
 * part of an object.
 */
public interface TransformI {
	/**
	 * @param in
	 *            the object to work with
	 * @param expression
	 *            eg xpath:[value=4].name
	 * @param SKIP
	 * @return the value extracted from the input, or SKIP if no such value was
	 *         present
	 */
	Object transform(Object in, String expression, Object SKIP);
}