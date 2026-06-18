package com.google.experimental.interactionlab.ai;

import com.theokanning.openai.completion.chat.ChatCompletionChoice;
import com.theokanning.openai.completion.chat.ChatCompletionRequest;
import com.theokanning.openai.completion.chat.ChatMessage;
import com.theokanning.openai.completion.chat.ChatMessageRole;
import com.theokanning.openai.service.OpenAiService;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class OpenAi {

  OpenAiService service;
  List<ChatMessage> messages = new ArrayList<>();
  ChatMessage summarizeStart = new ChatMessage(ChatMessageRole.SYSTEM.value(),
      "Can you summarize the following text into 30 words:");
  ChatMessage summarizeEnd = new ChatMessage(ChatMessageRole.SYSTEM.value(), "30-word summary:");

  String MODEL_SUMMARIZATION = "gpt-3.5-turbo";

  public static int MAX_TOKENS = 2048;

  public OpenAi(String apiKey) {
    service = new OpenAiService(apiKey);
  }

  public String summarize(ArrayList<String> text) {
    messages.clear();
    messages.add(summarizeStart);

    for (String t : text)
      messages.add(new ChatMessage(ChatMessageRole.USER.value(), t));
    messages.add(summarizeEnd);

    ChatCompletionRequest chatCompletionRequest = ChatCompletionRequest
        .builder()
        .model(MODEL_SUMMARIZATION)
        .messages(messages)
        .n(1)
        .maxTokens(MAX_TOKENS)
        .logitBias(new HashMap<>())
        .stop(null)
        .build();

    StringBuffer result = new StringBuffer();

    List<ChatCompletionChoice> choices = service.createChatCompletion(chatCompletionRequest)
        .getChoices();
    for (ChatCompletionChoice c : choices) {
//      System.out.println(c.getMessage().getContent());
//      System.out.println("------");
      result.append(c.getMessage().getContent());
    }

    return result.toString();
  }
}

/*

  public void test3()
  {
    OpenAiService service = new OpenAiService(API_KEY);

    System.out.println("Streaming chat completion...");
    final List<ChatMessage> messages = new ArrayList<>();
    final ChatMessage systemMessage = new ChatMessage(ChatMessageRole.SYSTEM.value(), "Can you summarize the following text:");
    messages.add(systemMessage);

    String[] dialogue = new String[]{"Lisa: Hi, I wonder if you could recommend a local restaurant?",
        "Anna: I guess it depends on how far you are willing to go?",
        "Lisa: I was thinking Santa Cruz, Capitola, Aptos and Soquel, but not farther than that",
        "Anna: Oh, I see. Let me think for a moment.",
        "Lisa: No worries, take your time!",
        "Anna: I think I would recommend Mentone in Aptos, if you like Italian food.",
        "Lisa: What are their specialties?",
        "Anna: They have pasta and pizza",
        "Lisa: How about drinks?",
        "Anna: Very good cocktails",
        "Lisa: Hmmm... how about something more American?",
        "Anna: Like traditional or modern?",
        "Lisa: Modern!",
        "Anna: Oh, then you may want to check out Persephone or the Home restaurant in Soquel",
        "Lisa: Do they serve local ingredients?",
        "Anna: Yes, Home even has its own garden with vegetables in the back!",
        "Lisa: Oh, that sounds amazing. Thank you very much!"};

    for (String line : dialogue)
      messages.add(new ChatMessage(ChatMessageRole.USER.value(), line));

    messages.add(new ChatMessage(ChatMessageRole.SYSTEM.value(), "Summary:"));

    ChatCompletionRequest chatCompletionRequest = ChatCompletionRequest
        .builder()
        .model("gpt-3.5-turbo")
        .messages(messages)
        .n(1)
        .maxTokens(2000)
        .logitBias(new HashMap<>())
        .build();

    service.createChatCompletion(chatCompletionRequest).getChoices().forEach(System.out::println);
/*    service.streamChatCompletion(chatCompletionRequest)
        .doOnError(Throwable::printStackTrace)
        .blockingForEach(System.out::println);

    service.shutdownExecutor();
  }

  public void test2() {
//    String token = System.getenv("OPENAI_TOKEN");
    String token = API_KEY;
    OpenAiService service = new OpenAiService(token, Duration.ofSeconds(30));

    System.out.println("\nCreating completion...");
    CompletionRequest completionRequest = CompletionRequest.builder()
        .model("ada")
        .prompt("Somebody once told me the world is gonna roll me")
        .echo(true)
        .user("testing")
        .n(3)
        .build();
    service.createCompletion(completionRequest).getChoices().forEach(System.out::println);

    System.out.println("\nCreating Image...");
    CreateImageRequest request = CreateImageRequest.builder()
        .prompt("A cow breakdancing with a turtle")
        .build();

    System.out.println("\nImage is located at:");
    System.out.println(service.createImage(request).getData().get(0).getUrl());

    System.out.println("Streaming chat completion...");
    final List<ChatMessage> messages = new ArrayList<>();
    final ChatMessage systemMessage = new ChatMessage(ChatMessageRole.SYSTEM.value(), "You are a dog and will speak as such.");
    messages.add(systemMessage);
    ChatCompletionRequest chatCompletionRequest = ChatCompletionRequest
        .builder()
        .model("gpt-3.5-turbo")
        .messages(messages)
        .n(1)
        .maxTokens(50)
        .logitBias(new HashMap<>())
        .build();

    service.streamChatCompletion(chatCompletionRequest)
        .doOnError(Throwable::printStackTrace)
        .blockingForEach(System.out::println);

    service.shutdownExecutor();
  }
*/
