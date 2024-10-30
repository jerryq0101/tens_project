chrome.runtime.onInstalled.addListener(() => {
        chrome.action.setBadgeText({
          text: "OFF",
        });
});

chrome.action.onClicked.addListener(async (tab) => {
        const prevState = await chrome.action.getBadgeText({ tabId: tab.id });
        // Next state will always be the opposite
        const nextState = prevState === 'ON' ? 'OFF' : 'ON';

        console.log("SEND DATA");
        chrome.runtime.sendNativeMessage(
                'com.jerryq0101.tens_project_communication',
                {state: nextState == "ON" ? 1 : 0},
                function (response) {
                        console.log("Received " + response.on);
                }
        );

        // Set the action badge to the next state
        await chrome.action.setBadgeText({
                text: nextState,
        });
});


