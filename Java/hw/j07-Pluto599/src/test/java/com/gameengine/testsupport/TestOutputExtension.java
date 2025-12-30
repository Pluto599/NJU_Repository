package com.gameengine.testsupport;

import org.junit.jupiter.api.extension.BeforeEachCallback;
import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.TestWatcher;

import java.util.Optional;

public class TestOutputExtension implements BeforeEachCallback, TestWatcher {
	private static final ExtensionContext.Namespace NS = ExtensionContext.Namespace.create(TestOutputExtension.class);
	private static final String START_NANOS = "startNanos";

	@Override
	public void beforeEach(ExtensionContext context) {
		context.getStore(NS).put(START_NANOS, System.nanoTime());
		System.out.println("\n==== TEST START ====\n" + describe(context));
	}

	@Override
	public void testSuccessful(ExtensionContext context) {
		System.out.println("==== TEST PASS  ====\n" + describeWithDuration(context));
	}

	@Override
	public void testFailed(ExtensionContext context, Throwable cause) {
		System.out.println("==== TEST FAIL  ====\n" + describeWithDuration(context));
		System.out.println("Failure: " + (cause == null ? "<null>" : cause.toString()));
	}

	@Override
	public void testAborted(ExtensionContext context, Throwable cause) {
		System.out.println("==== TEST ABORT ====\n" + describeWithDuration(context));
		System.out.println("Aborted: " + (cause == null ? "<null>" : cause.toString()));
	}

	@Override
	public void testDisabled(ExtensionContext context, Optional<String> reason) {
		System.out.println("==== TEST DISABLED ====\n" + describe(context) + "\nReason: " + reason.orElse("<none>"));
	}

	private static String describe(ExtensionContext context) {
		String displayName = context.getDisplayName();
		String className = context.getTestClass().map(Class::getName).orElse("<no-class>");
		String methodName = context.getTestMethod().map(m -> m.getName()).orElse("<no-method>");
		return "DisplayName: " + displayName + "\nTest: " + className + "#" + methodName;
	}

	private static String describeWithDuration(ExtensionContext context) {
		Long start = context.getStore(NS).get(START_NANOS, Long.class);
		double ms = 0.0;
		if (start != null) {
			ms = (System.nanoTime() - start) / 1_000_000.0;
		}
		return describe(context) + String.format("\nDuration: %.2f ms", ms);
	}
}
