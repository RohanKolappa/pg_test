package com.barco.nimbus.license.test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.*;

import java.util.LinkedList;
import java.util.List;

import org.junit.Test;

public class MockTest {

//	@Test
	public void testMockTest()
	{

		//mock creation
		List mockedList = mock(List.class);

		//using mock object
		mockedList.add("one");
		System.out.println(mockedList.size());
		mockedList.clear();

		System.out.println(mockedList.size());
		//verification
		verify(mockedList).add("one");
		System.out.println(mockedList.size());
		verify(mockedList).clear();
		System.out.println(mockedList.size());
		
		//You can mock concrete classes, not only interfaces
		LinkedList l = mock(LinkedList.class);

		//stubbing
		when(l.get(0)).thenReturn("first");
		when(l.get(1)).thenThrow(new RuntimeException());

		//following prints "first"
		System.out.println(l.get(0));

		//following throws runtime exception
//		System.out.println(l.get(1));

		//following prints "null" because get(999) was not stubbed
		System.out.println(l.get(999));
		 
		//Although it is possible to verify a stubbed invocation, usually it's just redundant
		//If your code cares what get(0) returns then something else breaks (often before even verify() gets executed).
		//If your code doesn't care what get(0) returns then it should not be stubbed. Not convinced? See here.
		verify(l).get(0);
		
		
		//using mock 
		mockedList.add("once");

		mockedList.add("twice");
		mockedList.add("twice");

		mockedList.add("three times");
		mockedList.add("three times");
		mockedList.add("three times");

		//following two verifications work exactly the same - times(1) is used by default
		verify(mockedList).add("once");
		verify(mockedList, times(1)).add("once");

		//exact number of invocations verification
		verify(mockedList, times(2)).add("twice");
		verify(mockedList, times(3)).add("three times");

		//verification using never(). never() is an alias to times(0)
		verify(mockedList, never()).add("never happened");

		//verification using atLeast()/atMost()
		verify(mockedList, atLeastOnce()).add("three times");
		verify(mockedList, atLeast(2)).add("five times");
		verify(mockedList, atMost(5)).add("three times");

	}
}
