package com.barco.nimbus.module.api;

import java.util.List;

/**
 * General purpose key/value store.
 */
public interface RegistryServiceI {
	/**
	 * @param arg
	 *            the first n-1 elements are considered key, the n-th item is
	 *            the value.
	 */
	void set(Iterable<List<String>> arg);

	String get(List<String> arg);

	Iterable<List<String>> get(String filter);
}
